require File.join(File.dirname(__FILE__), "..", "spec_helper")

describe "tidy class methods" do

  it "should have a new method" do
    tidy = Tidy.new
  end

  it "should parse a string" do
    tidy = Tidy.new
    result = tidy.parse("<html><body>Hello World!</body></html>")
    puts result.inspect
  end

end
