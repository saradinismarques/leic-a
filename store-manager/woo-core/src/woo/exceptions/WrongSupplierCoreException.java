package woo.exceptions;

/** Exception for reporting wrong supplier/product associations. */
public class WrongSupplierCoreException extends Exception {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202012250054L;

  /** Supplier key. */
  private String _skey;

  /** Product key. */
  private String _pkey;

  /** 
   * @param skey supplier key.
   * @param pkey product key. 
   */
  public WrongSupplierCoreException(String skey, String pkey) {
    _skey = skey;
    _pkey = pkey;
  }

  public String getPKey() { 
    return _pkey;
  }
}
