import com.rabbitmq.client.*;
import com.rabbitmq.client.ConnectionFactory;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.Channel;
import java.io.*;

public class Client {
  static public void main(String[] args) throws Exception, IOException {
    ConnectionFactory factory = new ConnectionFactory();
    factory.setHost("polluks.cs.put.poznan.pl");
    factory.setPort(6673);
    Connection connection = factory.newConnection();
    Channel channel = connection.createChannel();
    String name = channel.queueDeclare("", false, false, true, null).getQueue();
    channel.queueBind(name, "Xyz", "");
    while (true) {
      GetResponse resp = channel.basicGet(name, true);
      if( resp != null ){
        String message = new String(resp.getBody(), "UTF-8");
        System.out.println(" [x] Received '" + message + "'");
      }
    }
    // channel.close();
    // connection.close();
  }
}
