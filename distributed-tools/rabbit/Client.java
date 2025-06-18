import com.rabbitmq.client.*;
import com.rabbitmq.client.ConnectionFactory;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.Channel;
import java.io.*;

public class Client {
  static public void main(String[] args) throws Exception, IOException {
    ConnectionFactory factory = new ConnectionFactory();
    factory.setHost("localhost");
    factory.setPort(6677);
    Connection connection = factory.newConnection();
    Channel channel = connection.createChannel();
    channel.queueDeclare("kolejka1", false, false, false, null);
    GetResponse resp = channel.basicGet("kolejka1", false);
    if( resp != null ){
      String message = new String(resp.getBody(), "UTF-8");
      System.out.println(" [x] Received '" + message + "'");
    }
    channel.close();
    connection.close();
  }
}
