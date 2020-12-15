package woo.exceptions;

/** Exception for unknown client keys. */
public class UnknownClientKeyCoreException extends Exception {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202009192335L;

  /** Unknown key. */
  private String _key;

  /** @param key Unknown key to report. */
  public UnknownClientKeyCoreException(String key) {
    _key = key;
  }
}
