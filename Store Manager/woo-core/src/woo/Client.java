package woo;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import java.util.TreeMap;

import java.util.Map;

public class Client implements Serializable {

  private static final long serialVersionUID = 202009192206L;

  private String _key;
  private String _name;
  private String _address;
  private double _points;
  private double _salesValue;
  private double _payedSalesValue;
  private ArrayList<Notification> _notifications;
  private ClientState _state;

  public Client(String key, String name, String address) {
    _key = key;
    _name = name;
    _address = address;
    _points = 0;
    _notifications = new ArrayList<Notification>();
    _state = new NormalState(this);
  }

  public String getKey(){
    return _key;
  }
  
  public String getName(){
    return _name;
  }

  public String getAddress(){
    return _address;
  }

  public double getPoints(){
    return _points;
  }

  public void setPoints(double points) {
    _points = points;
  }

  public void setState(ClientState state) {
    _state = state;
  }

  public String getNotifications(){
    String str = "";

    for(Notification notification: _notifications)
      if(notification.getProduct().isClient(this))
        str += notification.toString();
    return str;
  }

  public void addNotification(String type, Product product) {
    _notifications.add(new Notification(type, product));
  }

  public void removeNotifications() {
   _notifications.clear(); 
  }

  public void update(String type, Product product){
    this.addNotification(type, product);
  }

  public void updateSalesValue(double saleValue) {
    _salesValue += saleValue;
  }

  public void pay(double payedPrice, int daysPassed) {
    _payedSalesValue += payedPrice;
    _state.updatePoints(payedPrice, daysPassed, _points);
    _state.updateState(daysPassed, _points);    
  }

  public double getPriceToPay(String timePeriod, double currentPrice, int daysPassed) {
    double priceToPay = 0;

    if(timePeriod == "P1")
      priceToPay = _state.getPriceP1(currentPrice, daysPassed);
    if(timePeriod == "P2")
      priceToPay = _state.getPriceP2(currentPrice, daysPassed);
    if(timePeriod == "P3")
      priceToPay = _state.getPriceP3(currentPrice, daysPassed);
    if(timePeriod == "P4")
      priceToPay = _state.getPriceP4(currentPrice, daysPassed);

    return priceToPay;  
  }

  @Override
  @SuppressWarnings("nls")
  public String toString() {
    String str = String.format("%s|%s|%s|%s|%d|%d", _key, _name, _address, _state.printState(), (int) Math.round(_salesValue), (int) Math.round(_payedSalesValue));
    return str + "\n";
  }
}

