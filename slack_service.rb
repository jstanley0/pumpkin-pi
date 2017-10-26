#!/usr/bin/env ruby

require 'highline/import'
require 'httparty'
require 'slack-rtmapi'
require 'json'
require 'date'
require 'colorize'
require_relative 'common.rb'

CONFIG_FILE = File.expand_path("~/.slack-auth")
PUMPKIN_PI_CHANNEL = 'C0DJ25DGF'

$slack_login_time = nil

class SlackAPI
  class Error < RuntimeError; end

  class Http
    include HTTParty
    base_uri 'https://slack.com'
    debug_output $stderr if ENV['SLACK_DEBUG'].to_i == 1
  end

  def self.get(path, url_params = {})
    response = Http.get(path, query: url_params.merge(token: $config[:slack_token]))
    $slack_login_time ||= DateTime.httpdate(response.headers['Date']).to_time.to_i
    raise Error.new("#{response.code} #{response.message}") unless response.code == 200
    json = JSON.parse(response.body)
    raise Error.new(response["error"]) unless response["ok"]
    json
  end
end


def check_config
  $config = {}
  config_changed = false
  config_file = File.new(CONFIG_FILE, File::CREAT|File::RDWR, 0600)
  raise "Permissions for #{CONFIG_FILE} are too liberal" if config_file.stat.world_readable?
  data = config_file.read
  unless data.empty?
    $config = JSON.parse(data).tap do |h|
      h.keys.each { |k| h[k.to_sym] = h.delete(k) }
    end
  end

  # log in to slack
  loop do
    if $config[:slack_token]
      begin
        result = SlackAPI.get("/api/auth.test")
        $slack_user_id = result["user_id"]
        puts "Logged into Slack"
        break
      rescue SlackAPI::Error => e
        puts "Slack login failed: #{e.message}"
        $config.delete(:slack_token)
      end
    end
    if !$config[:slack_token]
      config_changed = true
      $config[:slack_token] = ask("Paste your Slack token: ")
    end
  end

  # save config
  if config_changed
    config_file.rewind
    config_file.truncate(0)
    config_file.write $config.to_json
  end
end

def open_im_channel(user_id)
  json = SlackAPI.get "/api/im.open", user: user_id
  json["channel"]["id"]
end

def send_in_channel(channel, text)
  SlackAPI.get "/api/chat.postMessage", channel: channel, username: 'Pumpkin Pi', as_user: false, text: text,
               unfurl_links: false, unfurl_media: false, icon_emoji: ":jack_o_lantern:"
end

# send a direct message to the caller
def pm(message, text)
  send_in_channel(open_im_channel($slack_user_id), text)
end

# reply to the message in channel
def reply(message, text)
  send_in_channel(message["channel"], text)
end


# ...

$slack_user_id = nil
$pi = PiWrapper.new

check_config

$slack_client = SlackRTM::Client.new(websocket_url: SlackRTM.get_url(token: $config[:slack_token]))
$slack_client.on(:message) do |data|
  if data["type"] == "message" && data["channel"] == PUMPKIN_PI_CHANNEL
    next if data['subtype'] == 'bot_message'
    next if data['ts'].to_f < $slack_login_time
    text = data["text"]
    if text =~ /\Asay(?::([^: ]+))?(?::([^: ]+))?(?::([^: ]+))?(?::([^: ]+))? (.+)\z/i
      $pi.color('loud', $1, $3, $4) unless $3.nil? || $3.empty?
      reply data, $pi.say($5, $1, $2)
    elsif text =~ /\Aplay(?::([^: ]+))?(?::([^: ]+))?(?::([^: ]+))? (.+)\z/i
      $pi.color('loud', $1, $2, $3) unless $2.nil? || $2.empty?
      reply data, $pi.play($4, $1)
    elsif text =~ /\Acandle(?::([^: ]+))? (.+)\z/i
      result = if %w(off on).include?($2)
        $pi.candle($2)
      else
        $pi.color('bright', $1, $2)
      end
      reply data, result
    elsif text =~ /\Arandom(?::([^: ]+))?\z/i
      reply data, $pi.play(nil, $1)
    elsif text =~ /\Ahh\z/i
      reply data, $pi.say('Happy Halloween!')
    elsif text =~ /\Ayw\z/i
      reply data, $pi.say("You're welcome! Don't eat it all tonight!")
    elsif text =~ /\Ahello\z/i
      reply data, $pi.say("Hello! I'm the pumpkin down here.")
    end
  end
end

puts "Connected to Slack Real Time Messaging API"

$slack_client.main_loop

