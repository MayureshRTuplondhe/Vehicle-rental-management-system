package Thread;

//implement runable ---
class threadRUN implements Runnable{
    @Override
    public void run(){
        System.out.println("Thread is running...");
    }
}

class threadTERM implements Runnable{
    @Override
    public void run(){
        System.out.println("Thread is terminating...");
    }
}

public class impelment{
    public static void main(String[] args) {
        threadRUN r = new threadRUN();
        Thread t1 = new Thread(r);

        threadTERM t = new threadTERM();
        Thread t2 = new Thread(t);

        t1.start();
        t2.start();
    }
}