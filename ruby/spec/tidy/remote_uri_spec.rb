require File.join(File.dirname(__FILE__), "..", "spec_helper")
require 'open-uri'

describe "tidy class methods" do

  subject { Tidy.new }

  it "should parse the google.com uri" do
    uri = open("http://www.google.com")
    page = uri.read
    errors, html = subject.parse(page)
  end

end