require File.join(File.dirname(__FILE__), "..", "spec_helper")

describe "tidy compatibility methods" do

  subject { Tidy.new }

  it "should be defined" do
    defined?(Tidy).should be_true
  end

  it "should have an open method" do
    Tidy.should respond_to :open
  end

  it "should have a clean method" do
    subject.should respond_to :clean
  end

  it "should have an errors attribute" do
    subject.should respond_to :errors
  end

  it "should have a path attribute" do
    Tidy.should respond_to :path
    Tidy.path = "hello world"
    Tidy.path.should == "hello world"
  end

  it "should parse a string" do
    tidy = Tidy.open({}) do |tidy|
      xml = tidy.clean("<html><body>String</body></html>")
      xml.should_not be_empty
    end
  end

end
