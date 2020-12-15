package woo.exceptions;

/** Exception thrown when a client key is duplicated. */
public class DuplicateClientKeyCoreException extends Exception {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202011071508L;

  /** Client key. */
  private String _key;

  /** @param key the duplicated key */
  public DuplicateClientKeyCoreException(String key) {
    _key = key;
  }
}
