package woo.exceptions;

/** Exception for unknown service types. */
public class UnknownServiceTypeCoreException extends Exception {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202011072108L;

  /** Unknown type. */
  private String _type;

  /** @param type Unknown type to report. */
  public UnknownServiceTypeCoreException(String type) {
    _type = type;
  }
}
