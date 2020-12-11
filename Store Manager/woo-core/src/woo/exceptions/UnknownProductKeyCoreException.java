package woo.exceptions;

/** Exception for unknown product keys. */
public class UnknownProductKeyCoreException extends Exception {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202011071908L;

  /** Unknown key. */
  private String _key;

  /** @param key Unknown key to report. */
  public UnknownProductKeyCoreException(String key) {
    _key = key;
  }

  public String getProductKey() { 
    return _key;
  }
}
