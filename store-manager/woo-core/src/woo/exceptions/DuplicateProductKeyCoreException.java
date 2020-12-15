package woo.exceptions;

/** Exception thrown when a client key is duplicated. */
public class DuplicateProductKeyCoreException extends Exception {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202011071608L;

  /** Product key. */
  private String _key;

  /** @param key the duplicated key */
  public DuplicateProductKeyCoreException(String key) {
    _key = key;
  }
}
