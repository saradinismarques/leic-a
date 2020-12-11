package woo.exceptions;

/** Exception thrown when a supplier key is duplicated. */
public class DuplicateSupplierKeyCoreException extends Exception {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202011071708L;

  /** Supplier key. */
  private String _key;

  /** @param key the duplicated key */
  public DuplicateSupplierKeyCoreException(String key) {
    _key = key;
  }
}
