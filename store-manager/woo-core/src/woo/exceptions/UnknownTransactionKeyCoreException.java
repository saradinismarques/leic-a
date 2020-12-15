package woo.exceptions;

/** Exception for unknown transaction keys. */
public class UnknownTransactionKeyCoreException extends Exception {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202009192008L;

  /** Unknown key. */
  private int _key;

  /** @param key Unknown key to report. */
  public UnknownTransactionKeyCoreException(int key) {
    _key = key;
  }
}
