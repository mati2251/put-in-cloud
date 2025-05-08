import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;

public class Client {

  public static void main(String[] args) throws Exception {
    if (args.length != 2) {
      System.out.println("Usage: java Client <action> <value>");
      return;
    }

    Registry registry = LocateRegistry.getRegistry();
    Counter server = (Counter) registry.lookup("Counter");

    String action = args[0];
    int value = Integer.parseInt(args[1]);
    if (action.equals("up")) {
      int newValue = server.increment(value);
      System.out.println("Incremented value: " + newValue);
    } else if (action.equals("down")) {
      int newValue = server.decrement(value);
      System.out.println("Decremented value: " + newValue);
    } else {
      System.out.println("Invalid action. Use 'up' or 'down'.");
      return;
    }
  }
}
