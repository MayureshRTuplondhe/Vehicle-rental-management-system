package Thread;

//extend thread ---
class thread extends Thread{
    thread(String name){
        super(name);
    }

    @Override 
    public void run(){
        try {
        for(int i=1 ; i<=5; i++){
            System.out.println(getName() + " is Running");
        }
           Thread.sleep(3000);
           System.out.println("THREAD FINISHING");
        } catch (Exception e) {
            System.out.println(e);
        }

    // for(int i=1 ; i<=5; i++){
    //         System.out.println(getName() + " is Running");
    //     }
    // Thread.yield();

}
}
public class extend{
    public static void main(String[] args) {
        thread t1 = new thread("Thread 1");
        thread t2 = new thread("Thread 2");
        t1.start();
        t2.start();
    }
}
