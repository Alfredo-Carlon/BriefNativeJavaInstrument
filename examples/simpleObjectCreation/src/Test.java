public class Test
{
	public void sayHi()
	{
		System.out.println("Hiii from Test");
	}

	public static void main(String args[]){
		Test t = new Test();
		t.sayHi();
		System.out.println(t.toString());
	}	
	
}
