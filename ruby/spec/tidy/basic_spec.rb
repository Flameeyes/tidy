require File.join(File.dirname(__FILE__), "..", "spec_helper")

describe "tidy module methods" do

  it "should have a test method" do
    Tidy.test.should == 'hello world'
  end

end

