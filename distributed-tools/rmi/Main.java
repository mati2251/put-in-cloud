import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;

public class Main {
    public static void main(String[] args) throws RemoteException {
        System.out.println("Server!"); 
        Counter server = new CounterImpl();
        Counter stub = (Counter) UnicastRemoteObject.exportObject((Counter)server, 0);
        Registry registry = LocateRegistry.createRegistry(1099);
        registry.rebind("Counter", stub);
    }
}
