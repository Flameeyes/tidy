require File.join(File.dirname(__FILE__), "..", "spec_helper")

describe "tidy class methods" do

  subject { Tidy.new }

  it "should have a new method" do
    Tidy.should respond_to :new
  end

  it "should parse a string" do
    Tidy.new() do |tidy|
      errors, html = tidy.clean("<html><body>String</body></html>")
      errors.should_not be_nil
      html.should_not be_nil
    end
  end

  it "should parse a file" do
    file = File.new(File.join(File.dirname(__FILE__),'test1.html'))
    Tidy.new() do |tidy|
      errors, html = tidy.clean(file)
      errors.should_not be_nil
      html.should_not be_nil
    end
  end

  it "should respond to access" do
    subject.should respond_to(:access)
  end

  it "should be able to parse more than one string consecutively" do
    Tidy.new do |tidy|
      errors1, html1 = tidy.clean("<html><body>String</body></html>")
      errors2, html2 = tidy.clean("<html><head><title>hello</title></head><body>String</body></html>")
    end
  end

end
