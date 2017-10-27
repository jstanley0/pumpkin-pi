#!/usr/bin/env ruby

require 'sinatra'
require 'colorize'
require_relative 'common.rb'

$pi = PiWrapper.new

get '/' do
    erb :form
end

post '/say' do
    channel = params[:channel] || 's'
    if params[:color]
        $pi.color 'loud', channel, params[:color], params[:color2]    
    end
    $pi.say params[:text], channel, params[:voice]
end

post '/play' do
    channel = params[:channel] || 's'
    if params[:color]
        $pi.color 'loud', channel, params[:color], params[:color2]    
    end
    $pi.play params[:text], channel
end

post '/candle' do
    $pi.candle([1, '1', 'yes', 'on', 'true'].include?(params[:status]) ? 'on' : 'off')
end

post '/colors' do
    channel = params[:channel] || 's'
    $pi.color 'dark', channel, params[:dark], params[:dark2] if params[:dark]
    $pi.color 'bright', channel, params[:bright], params[:bright2] if params[:bright]
    $pi.color 'silent', channel, params[:silent], params[:silent2] if params[:silent]
    $pi.color 'loud', channel, params[:loud], params[:loud2] if params[:loud]
    'colors updated'
end

