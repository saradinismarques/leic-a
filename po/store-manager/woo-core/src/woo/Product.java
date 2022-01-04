package woo;

import java.io.Serializable;
import java.util.*;

public class Product implements Serializable {

  private static final long serialVersionUID = 202009200106L;

  private String _key;
  private Supplier _supplier;
  private int _price;
  private int _criticalLevel;
  private int _stock;
  private final int _valueN;
  private Map<String, Client> _clients = new TreeMap<String, Client>();

  public Product(String key, Supplier supplier, int price, int criticalLevel, int stock, int valueN) {
    _key = key;
    _price = price;
    _criticalLevel = criticalLevel;
    _supplier = supplier;
    _stock = stock;
    _valueN = valueN;

  }

  public String getKey(){
    return _key;
  }

  public Supplier getSupplier(){
    return _supplier;
  }

  public int getPrice(){
    return _price;
  }

  public int getCriticalLevel() {
    return _criticalLevel;
  }

  public int getStock() {
    return _stock;
  }

  public void updateStock(int amount) {
    _stock += amount;
  }

  public int getValueN(){
    return _valueN;
  }

  public void setPrice(int price) {
    _price = price;
  }

  public void attach(Client client) {
    _clients.put(client.getKey(),client);
  }

  public boolean detach(Client client) {
    return _clients.remove(client.getKey(), client);
  }

  public void notify(String type) {
    for(Map.Entry<String, Client> client: _clients.entrySet())
      client.getValue().update(type, this);
  }

  public boolean toggle(Client client) {
    if(_clients.containsValue(client)) {
      this.detach(client);
      return false;

    } else {
      this.attach(client);
      return true;
    }
  }

  public boolean isClient(Client client){
    return _clients.containsValue(client);
  }

  @Override
  @SuppressWarnings("nls")
  public String toString() {
    String str = String.format("%s|%s|%d|%d|%d", _key, _supplier.getKey(), _price, _criticalLevel, _stock);
      return str;
  }
}
