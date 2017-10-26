#!/usr/bin/env ruby

require 'sinatra'
require 'colorize'
require_relative 'common.rb'

$pi = PiWrapper.new

get '/' do
end

post '/say' do
    $pi.say params[:text]
end
