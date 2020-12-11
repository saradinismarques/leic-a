package woo;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

public class Order implements Serializable {

	private static final long serialVersionUID = 202012061214L;

    private int _key;
    private Supplier _supplier;
    private List<Product> _products;
    private List<Integer> _amounts;
    private int _price;
    private int _paymentDate;

    public Order(int key, Supplier supplier, int initialDate) {
        _key = key;
        _supplier = supplier;
        _products = new ArrayList<Product>();
        _amounts = new ArrayList<Integer>();
        _paymentDate = initialDate;
        _price = 0;
    }

    public Supplier getSupplier() {
        return _supplier;
    }
    
    public String getProducts(){
        String str = "";

        for(int i = 0; i < _products.size(); i++) 
            str += String.format("%s|%d\n", _products.get(i).getKey(), _amounts.get(i));

        return str;
    }

    public int getPrice() {
        return _price;
    }
     
    public void addProduct(Product product, int amount){
        _products.add(product);
        _amounts.add(amount);
        _price += product.getPrice() * amount;
    }

    @Override
    @SuppressWarnings("nls")
    public String toString() {
        String str = String.format("%d|%s|%d|%d", _key, _supplier.getKey(), _price, _paymentDate);
        return str + "\n";
    }
}
