require 'colorize'

class PiWrapper
    PUMPKIN_PI_PATH = File.expand_path("./pumpkin-pi", File.dirname(__FILE__))
    SOUND_PATH = File.expand_path("./sounds", File.dirname(__FILE__))
    VOICE_PATH = "/tmp/pumpkin-pi.wav"

    def initialize
        @pi = IO.popen([PUMPKIN_PI_PATH, '--interactive'], 'w')
        Signal.trap('INT') do
            if @pi
                puts 'Shutting down pumpkin-pi...'
                send_pi 'exit'
                Process.wait @pi.pid
            end
            exit
        end
    end

    def say(text, channel = 's', voice = nil, color = nil, color2 = nil)
        channel = 's' if channel.nil? || channel.empty?
        cmd = ["espeak", "-w", VOICE_PATH]
        cmd += ["-v", voice] if voice
        puts cmd.join(' ').cyan
        IO.popen(cmd, 'w') { |io| io.puts(text) }
        send_colors(channel, color, color2)
        send_pi "play #{channel} #{VOICE_PATH}"
        text
    end
    
    def play(file = nil, channel = 's', color = nil, color2 = nil)
        channel = 's' if channel.nil? || channel.empty?
        file ||= ''
        file = Dir.entries(SOUND_PATH).select { |sound_file| sound_file.downcase.include? file.downcase }.sample
        if file
            send_colors(channel, color, color2)
            send_pi "play #{channel} #{SOUND_PATH}/#{file}"
            "playing \"#{file}\""
        else
            "couldn't find that sound"
        end
    end
    
    def candle(color)
        send_pi "color bright #{translate_color(color)}"
        "candle #{color}"
    end

    private
    
    COLORS = {
        "off" => "#000000",
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
        STDERR.puts command
        @pi.puts command
        @pi.flush
    end

    def send_colors(channel, color, color2)
        return if color.nil? || color.empty?
        case channel
        when 'l', 'r'
            send_pi "color loud #{channel} #{translate_color(color)}"
        else
            if color2 && !color2.empty?
                send_pi "color loud l #{translate_color(color)}"
                send_pi "color loud r #{translate_color(color2)}"
            else
                send_pi "color loud #{translate_color(color)}"
            end
        end
    end
end