package woo;

import java.io.Serializable;

public class Box extends Package implements Serializable { 
    /** Serial number for serialization. */
    private static final long serialVersionUID = 202009192106L;

    public Box(String key, type serviceType, Supplier supplier, int price, int criticalLevel, int stock) {
        super(key, serviceType, supplier, price, criticalLevel, stock, 5);
    }

    @Override
    @SuppressWarnings("nls")
    public String toString() {
        return "BOX|" + super.toString() + "\n";
    }   
}
