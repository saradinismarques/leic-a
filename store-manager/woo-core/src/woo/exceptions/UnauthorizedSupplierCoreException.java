package woo.exceptions;

/** Exception for reporting unauthorized supplier attempts. */
public class UnauthorizedSupplierCoreException extends Exception {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202012250054L;

  /** Unknown key. */
  private String _key;

  /** @param key unauthorized key to report. */
  public UnauthorizedSupplierCoreException(String key) {
    _key = key;
  }
}
