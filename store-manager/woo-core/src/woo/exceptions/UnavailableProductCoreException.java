package woo.exceptions;

/** Exception thrown when a client key is duplicated. */
public class UnavailableProductCoreException extends Exception {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 201709021324L;

  /** Product key. */
  private String _key;

  /** Requested amount. */
  int _requested;
  
  /** Available amount. */
  int _available;
  
  /** 
   * @param key the requested key
   * @param requested
   * @param available
   */
  public UnavailableProductCoreException(String key, int requested, int available) {
    _key = key;
    _requested = requested;
    _available = available;
  }

  public int getAvailable() {
    return _available;
  }
}
