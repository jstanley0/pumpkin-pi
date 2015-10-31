lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)

Gem::Specification.new do |s|
  s.name        = 'pumpkin-pi'
  s.version     = '0.0.1'
  s.date        = '2015-10-30'
  s.summary     = "A thing that lets you control a jack-o-lantern via Slack"
  s.description = "A serious waste of time"
  s.authors     = ["Jeremy Stanley"]
  s.email       = 'jstanley0@gmail.com'
  s.files       = `git ls-files`.split("\n")
  s.homepage    = 'http://github.com/jstanley0/pumpkin-pi'
  s.license     = 'Apache'
  s.bindir      = 'bin'
  s.executables << 'pumpkin-pi'
  s.required_ruby_version = '>= 1.9.3'
  s.add_dependency 'httparty', '~> 0'
  s.add_dependency 'slack-rtmapi', '1.0.0.rc4'
  s.add_dependency 'colorize', '~> 0.7'
end
