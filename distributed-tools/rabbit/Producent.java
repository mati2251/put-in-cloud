import com.rabbitmq.client.*;
import com.rabbitmq.client.ConnectionFactory;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.Channel;
import java.io.*;

public class Producent {
  static public void main(String[] args) throws Exception, IOException {
    ConnectionFactory factory = new ConnectionFactory();
    factory.setHost("localhost");
    factory.setPort(6677);
    Connection connection = factory.newConnection();
    Channel channel = connection.createChannel();
    String message = "Hello world";
    channel.queueDeclare("kolejka1", false, false, false, null);
    channel.basicPublish("", "kolejka1", null, message.getBytes("UTF-8"));
    channel.close();
    connection.close();
  }
}
