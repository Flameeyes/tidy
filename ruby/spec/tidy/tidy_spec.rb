require File.join(File.dirname(__FILE__), "..", "spec_helper")

describe "tidy class methods" do

  subject { Tidy.new }

  it "should have a new method" do
    Tidy.should respond_to :new
  end

  it "should parse a string" do
    errors, html = subject.parse("<html><body>String</body></html>")
    errors.should_not be_nil
    html.should_not be_nil
  end

  it "should parse a file" do
    file = File.new(File.join(File.dirname(__FILE__),'test1.html'))
    errors, html = subject.parse(file)
    errors.should_not be_nil
    html.should_not be_nil
  end

  it "should respond to access" do
    subject.should respond_to(:access)
  end

  it "should be able to parse more than one string consecutively" do
    errors1, html1 = subject.parse("<html><body>String</body></html>")
    errors2, html2 = subject.parse("<html><head><title>hello</title></head><body>String</body></html>")
    errors1.should_not == errors2
  end

end
