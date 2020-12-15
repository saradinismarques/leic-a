package woo;

import java.io.Serializable;

public class Sale implements Serializable{

	private static final long serialVersionUID = 202012061216L;

    private int _key;
    private Client _client;
    private Product _product;
    private int _amount;
    private int _paymentDeadline;
    private boolean _isPayed;
    private double _inicialPrice;
    private double _payedPrice;
    private int _initialDate;
    private int _paymentDate;

    public Sale(int key, Client client, int paymentDeadline, Product product, int amount, int initialDate) {
       _key = key;
       _client = client;
       _paymentDeadline = paymentDeadline;
       _product = product;
       _amount = amount;
       _isPayed = false;
       _inicialPrice = _product.getPrice() * amount;
       _client.updateSalesValue(_inicialPrice);
       _initialDate = initialDate;
    }

    public Client getClient() {
        return _client;
    }

    public int getPaymentDeadline() {
        return _paymentDeadline;
    }

    public Product getProdut() {
        return _product;
    }

    public boolean isPayed() {
        return _isPayed;
    }

    public double getPayedPrice() {
        return _payedPrice;
    }

    protected String getTimePeriod(int currentDate) {
        int valueN = _product.getValueN();
        
        if(_paymentDeadline - currentDate >= valueN)
          return "P1";
        else if(0 <= _paymentDeadline - currentDate && _paymentDeadline - currentDate < valueN)
          return "P2";
        else if(0 < currentDate - _paymentDeadline && currentDate - _paymentDeadline <= valueN)
          return "P3";
        else
          return "P4";
    }

    public double getPriceToPay(int currentDate) {
        String timePeriod = this.getTimePeriod(currentDate);
        double currentPrice = _product.getPrice() * _amount;
        int daysPassed = currentDate - _paymentDeadline;
        double priceToPay = _client.getPriceToPay(timePeriod, currentPrice, daysPassed);

        return priceToPay;
    }

    public void pay(int currentDate) {
        _paymentDate = currentDate;
        int daysPassed = currentDate - _paymentDeadline;
        _payedPrice = this.getPriceToPay(currentDate);
        _client.pay(_payedPrice, daysPassed);
        _isPayed = true;
    }

    @Override
    @SuppressWarnings("nls")
    public String toString() {
        String str = String.format("%d|%s|%s|%d|%d|%d|%d|%d", _key, _client.getKey(), _product.getKey(), _amount, (int) Math.round(_inicialPrice), (int) Math.round(_payedPrice), _paymentDeadline, _paymentDate);
        return str + "\n";
    }

    public String toString(int currentDate) {
        String str = String.format("%d|%s|%s|%d|%d|%d|%d", _key, _client.getKey(), _product.getKey(), _amount, (int) Math.round(_inicialPrice), (int) Math.round(this.getPriceToPay(currentDate)), _paymentDeadline);
        return str + "\n";
    }
} 
