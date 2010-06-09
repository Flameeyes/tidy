require File.join(File.dirname(__FILE__), "..", "spec_helper")

describe "tidy class methods" do

  it "should have a new method" do
    tidy = Tidy.new
  end

  it "should parse a string" do
    tidy = Tidy.new
    result = tidy.parse("<html><body>String</body></html>")
    puts result.inspect
  end

  it "should parse a file" do
    file = File.new(File.join(File.dirname(__FILE__),'test1.html'))
    tidy = Tidy.new
    result = tidy.parse(file)
    puts result.inspect
  end

end
