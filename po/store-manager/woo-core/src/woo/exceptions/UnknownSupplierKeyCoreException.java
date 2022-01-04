package woo.exceptions;

/** Exception for unknown supplier keys. */
public class UnknownSupplierKeyCoreException extends Exception {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202011072208L;

  /** Unknown key. */
  private String _key;

  /** @param key Unknown key to report. */
  public UnknownSupplierKeyCoreException(String id) {
    _key = id;
  }
}
