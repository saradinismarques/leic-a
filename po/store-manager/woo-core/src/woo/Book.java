package woo;

import java.io.Serializable;

public class Book extends Product implements Serializable {
    /** Serial number for serialization. */
    private static final long serialVersionUID = -5863226379882723342L;
    
    private String _title;
    private String _author;
    private String _isbn;

    public Book(String key, String title, String author, String isbn, Supplier supplier, int price, int criticalLevel, int stock) {
        super(key, supplier, price, criticalLevel, stock, 3);
        _title = title;
        _author = author;
        _isbn = isbn;
    }

    public String getTitle(){
        return _title;
    }

    public String getAuthor(){
        return _author;
    }

    public String getIsbn(){
        return _isbn;
    }

    @Override
    @SuppressWarnings("nls")
    public String toString() {
        String str = String.format("|%s|%s|%s", _title, _author, _isbn);
            return "BOOK|" + super.toString() + str + "\n";
  }   
}
