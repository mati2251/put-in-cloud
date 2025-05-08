import java.rmi.Remote;
import java.rmi.RemoteException;

interface Counter extends Remote {
  public int increment(int value) throws Exception;
  public int decrement(int value) throws Exception;
}
