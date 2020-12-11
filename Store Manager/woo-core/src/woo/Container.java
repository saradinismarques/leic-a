package woo;

import java.io.Serializable;

public class Container extends Package implements Serializable{
    /** Serial number for serialization. */
    private static final long serialVersionUID = 202009192306L;
    
    public enum level {B4, C4, C5, DL}
    private level _serviceLevel;

    public Container(String key, type serviceType, level serviceLevel, Supplier supplier, int price, int criticalLevel, int stock) {
        super(key, serviceType, supplier, price, criticalLevel, stock, 8);
        _serviceLevel = serviceLevel;
    }

    public level getServiceLevel(){
        return _serviceLevel;
    }

    @Override
    @SuppressWarnings("nls")
    public String toString() {
        String str = String.format("|%s", _serviceLevel);
            return "CONTAINER|" + super.toString() + str + "\n";
    }   
}
