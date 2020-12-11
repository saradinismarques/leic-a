package woo;

import java.io.Serializable;

public class Supplier implements Serializable {

    private static final long serialVersionUID = 202009200206L;
  
    private String _key;
    private String _name;
    private String _address;
    private boolean _isActive;
  
    public Supplier(String key, String name, String address) {
      _key = key;
      _name = name;
      _address = address;
      _isActive = true;
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

    public boolean isActive() {
      return _isActive;
    }

    public boolean toggleTransactions() {
      _isActive = !_isActive;
      return _isActive;
    }
    
    @Override
    @SuppressWarnings("nls")
    public String toString() {
      String state;

      if(_isActive)
        state = "SIM";
      else
        state = "NÃO";

      String str = String.format("%s|%s|%s|%s", _key, _name, _address, state);
      return str + "\n";
    }
}
