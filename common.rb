require 'colorize'

class PiWrapper
    PUMPKIN_PI_PATH = File.expand_path("./pumpkin-pi", File.dirname(__FILE__))
    SOUND_PATH = File.expand_path("./sounds", File.dirname(__FILE__))
    VOICE_PATH = "/tmp/pumpkin-pi.wav"

    def initialize
        @pi = IO.popen([PUMPKIN_PI_PATH, '--interactive'], 'w')
        Signal.trap('INT') do
            if @pi
                STDERR.puts 'Shutting down pumpkin-pi...'
                send_pi 'exit'
                Process.wait @pi.pid
            end
            exit
        end
    end

    def say(text, channel = 's', voice = nil)
        channel = 's' if channel.nil? || channel.empty?
        cmd = ["espeak", "-w", VOICE_PATH]
        cmd += ["-v", voice] if voice
        STDERR.puts cmd.join(' ').cyan
        IO.popen(cmd, 'w') { |io| io.puts(text) }
        send_pi "play #{channel} #{VOICE_PATH}"
        text
    end
    
    # file can be a partial name; if more than one match is found, a file will be chosen randomly
    def play(file = nil, channel = 's')
        channel = 's' if channel.nil? || channel.empty?
        file ||= ''
        file = Dir.entries(SOUND_PATH).select { |sound_file| sound_file.downcase.include? file.downcase }.sample
        if file
            send_pi "play #{channel} #{SOUND_PATH}/#{file}"
            "playing \"#{file}\""
        else
            "couldn't find that sound"
        end
    end
    
    # status is 'on' or 'off'
    def candle(status)
        return unless ['on', 'off'].include?(status)
        send_pi "candle #{status}"
    end

    # label is one of 'dark', 'bright' (candle colors) or 'silent', 'loud' (speech colors)
    # channel is one of 'l', 'r', or 's'; use 's' to set the color for both channels at once
    # in the 's' case, if one color is provided, it will be applied to both channels
    def color(label, channel, color, color2 = nil)
        return if color.nil? || color.empty?
        case channel
        when 'l', 'r'
            send_pi "color #{label} #{channel} #{translate_color(color)}"
        else
            if color2 && !color2.empty?
                send_pi "color #{label} l #{translate_color(color)}"
                send_pi "color #{label} r #{translate_color(color2)}"
            else
                send_pi "color #{label} #{translate_color(color)}"
            end
        end      
    end

    private
    
    COLORS = {
        "red" => "#ff0000",
        "orange" => "#ff6000",
        "yellow" => "#ffc000",
        "green" => "#00ff00",
        "cyan" => "#00ffff",
        "blue" => "#0000ff",
        "purple" => "#c000ff",
        "white" => "#ffffff"
    }

    def translate_color(color)
        COLORS[color] || color
    end

    def send_pi(command)
        STDERR.puts command.yellow
        @pi.puts command
        @pi.flush
        command
    end
end