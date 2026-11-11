package Thread;

public class Threadlifecycle {
    
    public static void main(String[] args) throws InterruptedException{
        
        Thread t1 = new Thread(()-> {
            try {
                System.out.println("Thread is running");
                Thread.sleep(3000);
                System.out.println("Thread resumed");
            } catch (InterruptedException e) {
                System.out.println("Thread Interuppted" + e);
            }
        });

        System.out.println("State: "+ t1.getState()); //new thread created

        t1.start();
        System.out.println("State: "+ t1.getState()); // when we do t1.start() it calls the code inside the run method

        Thread.sleep(500); // pauses currently executing state
        System.out.println("State: " + t1.getState());

        t1.join(); //indefinite waiting
        System.out.println("State: " + t1.getState());
    }
}