package woo;

import java.io.Serializable;

public class Notification implements Serializable {

	private static final long serialVersionUID = 202012061213L;

    private Product _product;
    private String _type;
    private int _price;

    public Notification(String type, Product product){
        _type = type;
        _product = product;
        _price = product.getPrice();
    }

    public Product getProduct() {
        return _product;
    }
    
    @Override
    @SuppressWarnings("nls")
    public String toString() {
      String str = String.format("%s|%s|%d", _type, _product.getKey(), _price);
        return str + "\n";
    }
}
