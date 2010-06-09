require File.join(File.dirname(__FILE__), "..", "spec_helper")
require 'open-uri'

describe "tidy class methods" do

  it "should parse the google.com uri" do
    tidy = Tidy.new
    errors, html = tidy.parse(open("http://www.google.com"))
  end

end