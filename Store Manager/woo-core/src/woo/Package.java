package woo;

import java.io.Serializable;

public class Package extends Product implements Serializable {
    /** Serial number for serialization. */
    private static final long serialVersionUID = 202012061215L;
    
    public enum type {NORMAL, AIR, EXPRESS, PERSONAL};
    private type _serviceType; 

    public Package(String key, type serviceType, Supplier supplier, int price, int criticalLevel, int stock, int valueN) {
        super(key, supplier, price, criticalLevel, stock, valueN);
        _serviceType = serviceType;
    }

    public type getServiceType(){
        return _serviceType;
    }

    @Override
    @SuppressWarnings("nls")
    public String toString() {
        String str = String.format("|%s", _serviceType);
        return super.toString() + str;
    }
    
}
