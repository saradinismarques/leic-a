package woo;

import java.io.Serializable;
import java.io.IOException;
import java.io.BufferedReader;
import java.io.FileReader;
import java.util.TreeMap;
import java.util.Map;
import java.util.List;   
import java.util.ArrayList;
import java.util.regex.*;
import woo.exceptions.*;

/**
 * Class Store implements a store.
 */
public class Store implements Serializable {

  /** Serial number for serialization. */
  private static final long serialVersionUID = 202009192006L;

  private double _availableBalance;
  private double _accoutingBalance;
	private int _date;
  private int _nextTransactionKey;
	private Map<String, Product> _products;
	private Map<String, Client> _clients;
	private Map<String, Supplier> _suppliers;
  private Map<Integer, Sale> _sales;
  private Map<Integer, Order> _orders;

  /**
   * Constructor.
   */
  public Store() {
    _availableBalance = 0;
    _accoutingBalance = 0;
    _date = 0;
		_nextTransactionKey = 0;
    _products = new TreeMap<String, Product>(String.CASE_INSENSITIVE_ORDER);
    _clients = new TreeMap<String, Client>(String.CASE_INSENSITIVE_ORDER);
    _suppliers = new TreeMap<String, Supplier>(String.CASE_INSENSITIVE_ORDER);
    _sales = new TreeMap<Integer, Sale>();
    _orders = new TreeMap<Integer, Order>();
  }

  /*-------------------------------------------------MAIN------------------------------------------------*/

  /**
   * Read the text input file splitting each line inte arguments to be registered.
	 * instances of the various possible types (books, DVDs, users).
   * @param txtfile filename to be loaded.
   * @throws IOException
   * @throws BadEntryException
   * @throws DuplicateProductKeyCoreException
   * @throws UnknownSupplierKeyCoreException
   * @throws UnknownServiceTypeCoreException
   * @throws UnknownServiceLevelCoreException
	 * @throws DuplicateClientKeyCoreException
	 * @throws DuplicateSupplierKeyCoreException
   * @throws UnknownClientKeyCoreException
   */
  public void importFile(String txtfile) throws IOException, BadEntryException, DuplicateProductKeyCoreException, UnknownSupplierKeyCoreException, UnknownServiceTypeCoreException, UnknownServiceLevelCoreException, DuplicateClientKeyCoreException, DuplicateSupplierKeyCoreException, UnknownClientKeyCoreException {
    BufferedReader reader = new BufferedReader(new FileReader(txtfile));
    String line;

    while ((line = reader.readLine()) != null) {
      String[] fields = line.split("\\|");
      registerFromFields(fields);
    }
    reader.close();
  }

  /**
	 * Register each field according to its type.
	 * @param fields
   * @throws IOException
   * @throws BadEntryException
   * @throws DuplicateProductKeyCoreException
   * @throws UnknownSupplierKeyCoreException
   * @throws UnknownServiceTypeCoreException
   * @throws UnknownServiceLevelCoreException
	 * @throws DuplicateClientKeyCoreException
	 * @throws DuplicateSupplierKeyCoreException
   * @throws UnknownClientKeyCoreException
   */
  public void registerFromFields(String[] fields) throws DuplicateProductKeyCoreException, UnknownSupplierKeyCoreException, UnknownServiceTypeCoreException, UnknownServiceLevelCoreException, DuplicateClientKeyCoreException, DuplicateSupplierKeyCoreException, UnknownClientKeyCoreException {
    Pattern patBox = Pattern.compile("^(BOX)");
    Pattern patContainer = Pattern.compile("^(CONTAINER)");
    Pattern patBook = Pattern.compile("^(BOOK)");
    Pattern patClient = Pattern.compile("^(CLIENT)");
    Pattern patSupplier = Pattern.compile("^(SUPPLIER)");

    if (patBox.matcher(fields[0]).matches()) {
      registerBox(fields[1], fields[2], fields[3], Integer.parseInt(fields[4]), Integer.parseInt(fields[5]), Integer.parseInt(fields[6]));
    } else if (patContainer.matcher(fields[0]).matches()) {
      registerContainer(fields[1], fields[2], fields[3], fields[4], Integer.parseInt(fields[5]), Integer.parseInt(fields[6]), Integer.parseInt(fields[7]));
    } else if (patBook.matcher(fields[0]).matches()) {
      registerBook(fields[1], fields[2], fields[3], fields[4], fields[5], Integer.parseInt(fields[6]), Integer.parseInt(fields[7]), Integer.parseInt(fields[8]));
    } else if (patClient.matcher(fields[0]).matches()) {
      registerClient(fields[1], fields[2], fields[3]);
    } else if (patSupplier.matcher(fields[0]).matches()) {
      registerSupplier(fields[1], fields[2], fields[3]);
    } 
  }

  /**
    * @return current date.
    */
  public int getDate() {
    return _date;
  }

  /**
	 * Advances date if valid.
	 * @param daysToAdvance number of days to advance.
   * @throws InvalidDateCoreException
	 * @return nothing.
	 */
  protected void advanceDate(int daysToAdvance) throws InvalidDateCoreException {
		if(daysToAdvance <= 0){
			throw new InvalidDateCoreException(daysToAdvance);
		}
		_date += daysToAdvance;
  }

  public int getAvailableBalance() {
    return (int) Math.round(_availableBalance);
  }

  public int getAccountingBalance() {
    double currentBalance = _accoutingBalance;

    for(Map.Entry<Integer, Sale> sale: _sales.entrySet())
      if(!sale.getValue().isPayed())
        currentBalance += sale.getValue().getPriceToPay(this.getDate());
        
    return (int) Math.round(currentBalance);
  }

  public void updateAvailableBalance(double amount) {
    _availableBalance += amount;
  }

  public void updateAccountingBalance(double amount) {
    _accoutingBalance +=amount;
  }

  /*-------------------------------------------------PRODUCTS------------------------------------------------*/

  /**
   * Return all the products in the store.
   * 
   * @return a tree map with all the products alphabetically ordered by key.
   */
  protected String getAllProducts(){
    String str = "";

    for(Map.Entry<String, Product> product: _products.entrySet())
      str += product.getValue().toString();

    return str;
  }

  /**
	 * Register new box and adds it to the store.
	 * @param key
	 * @param serviceType
   * @param supplierKey
   * @param price
   * @param criticalLevel
   * @param stock
	 * @throws DuplicateProductKeyCoreException
   * @throws UnknownSupplierKeyCoreException
   * @throws UnknownServiceTypeCoreException
	 * @return nothing.
	 */

  protected void registerBox(String key, String serviceType, String supplierKey, int price, int criticalLevel, int stock) throws DuplicateProductKeyCoreException, UnknownSupplierKeyCoreException, UnknownServiceTypeCoreException {
    if(_products.containsKey(key))
      throw new DuplicateProductKeyCoreException(key);

    if(!_suppliers.containsKey(supplierKey))
      throw new UnknownSupplierKeyCoreException(supplierKey);

    Box.type enumServiceType = null;

    try {
      enumServiceType = Box.type.valueOf(serviceType);   
    } catch (IllegalArgumentException e) {  
      throw new UnknownServiceTypeCoreException(supplierKey);
    }
    Product box = new Box(key, enumServiceType, _suppliers.get(supplierKey), price, criticalLevel, stock);
     _products.put(key, box);

    for(Map.Entry<String, Client> client: _clients.entrySet())
      box.attach(client.getValue());  
  }

  /**
	 * Register new container and adds it to the store.
	 * @param key
	 * @param serviceType
   * @param serviceLevel
   * @param supplierKey
   * @param price
   * @param criticalLevel
   * @param stock
	 * @throws DuplicateProductKeyException
   * @throws UnknownSupplierKeyCoreException
   * @throws UnknownServiceTypeCoreException
   * @throws UnknownServiceLevelCoreException
	 * @return nothing.
	 */
  protected void registerContainer(String key, String serviceType, String serviceLevel, String supplierKey, int price, int criticalLevel, int stock) throws DuplicateProductKeyCoreException, UnknownSupplierKeyCoreException, UnknownServiceTypeCoreException, UnknownServiceLevelCoreException {
    if(_products.containsKey(key))
      throw new DuplicateProductKeyCoreException(key);

    if(!_suppliers.containsKey(supplierKey))
      throw new UnknownSupplierKeyCoreException(supplierKey);
    
    Container.type enumServiceType = null;
    Container.level enumServiceLevel = null; 

    try {
      enumServiceType = Container.type.valueOf(serviceType);   
    } catch (IllegalArgumentException e) {  
      throw new UnknownServiceTypeCoreException(serviceType);
    }
    try {
      enumServiceLevel = Container.level.valueOf(serviceLevel); 
    } catch(IllegalArgumentException e){
      throw new UnknownServiceLevelCoreException(serviceLevel);
    }

    Product container = new Container(key, enumServiceType, enumServiceLevel, _suppliers.get(supplierKey), price, criticalLevel, stock);
    _products.put(key, container);

    for(Map.Entry<String, Client> client: _clients.entrySet()) 
      container.attach(client.getValue());
  }

  /**
	 * Register new book and adds it to the store.
	 * @param key
	 * @param title
   * @param author
   * @param isbn
   * @param supplierKey
   * @param price
   * @param criticalLevel
   * @param stock
	 * @throws DuplicateProductKeyCoreException
   * @throws UnknownSupplierKeyCoreException
	 * @return nothing.
	 */
  protected void registerBook(String key, String title, String author, String isbn, String supplierKey, int price, int criticalLevel, int stock) throws DuplicateProductKeyCoreException, UnknownSupplierKeyCoreException {
    if(_products.containsKey(key))
      throw new DuplicateProductKeyCoreException(key);

    if(!_suppliers.containsKey(supplierKey))
      throw new UnknownSupplierKeyCoreException(supplierKey);

    Product book = new Book(key, title, author, isbn, _suppliers.get(supplierKey), price, criticalLevel, stock);
    _products.put(key, book);

    for(Map.Entry<String, Client> client: _clients.entrySet())
      book.attach(client.getValue());
  }

  protected void changePrice(String key, int newPrice) throws UnknownProductKeyCoreException {
    if(!_products.containsKey(key))
      throw new UnknownProductKeyCoreException(key);

    if(newPrice <= 0) 
      return;

    int oldPrice = _products.get(key).getPrice();

    _products.get(key).setPrice(newPrice);           

    if(newPrice < oldPrice) 
      _products.get(key).notify("BARGAIN");

  }

  /*-------------------------------------------------CLIENTS------------------------------------------------*/

  /**
   * Get the client with the given key.
   * @param key the client's key.
   * @throws UnknownClientKeyCoreException
   * @return the client if it exists.
   */
  protected String getClient(String key) throws UnknownClientKeyCoreException {
    if(!_clients.containsKey(key))
      throw new UnknownClientKeyCoreException(key);

    String str = "";

    str = _clients.get(key).toString() + _clients.get(key).getNotifications();
    _clients.get(key).removeNotifications();

    return str;
  }

  /**
   * Return all the clients in the store.
   * 
   * @return a tree map with all the clients alphabetically ordered by key.
   */
  protected String getAllClients(){
    String str = "";

    for(Map.Entry<String, Client> client: _clients.entrySet())
      str += client.getValue().toString();

    return str;
  }

   /**
	 * Register new client and adds it to the store.
	 * @param key
	 * @param name
   * @param address
	 * @throws DuplicateClientKeyCoreException
	 * @return nothing.
	 */
  protected void registerClient(String key, String name, String address) throws DuplicateClientKeyCoreException {
    if(_clients.containsKey(key))
      throw new DuplicateClientKeyCoreException(key);

    Client client = new Client(key, name, address);
    _clients.put(key, client);

    for(Map.Entry<String, Product> product: _products.entrySet())
      product.getValue().attach(client);
  }

  protected boolean toggleNotifications(String clientKey, String productKey) throws UnknownClientKeyCoreException, UnknownProductKeyCoreException {
    if(!_clients.containsKey(clientKey))
      throw new UnknownClientKeyCoreException(clientKey);
    if(!_products.containsKey(productKey))
      throw new UnknownProductKeyCoreException(productKey);

    return _products.get(productKey).toggle(_clients.get(clientKey));   
  }

  protected String getClientTransactions(String key) throws UnknownClientKeyCoreException {
    if(!_clients.containsKey(key))
      throw new UnknownClientKeyCoreException(key);
    String str = "";  
    for(Map.Entry<Integer, Sale> sale: _sales.entrySet())
      if(sale.getValue().getClient().getKey().equals(key)) 
        str += sale.getValue().toString();  

    return str;
  }

  /*-------------------------------------------------SUPPLIERS------------------------------------------------*/

  /**
   * Return all the suppliers in the store.
   * 
   * @return a tree map with all the suppliers alphabetically ordered by key.
   */
  protected String getAllSuppliers(){
    String str = "";

    for(Map.Entry<String, Supplier> supplier: _suppliers.entrySet())
      str += supplier.getValue().toString();

    return str;
  }

   /**
	 * Register new supplier and adds it to the store.
	 * @param key
	 * @param name
   * @param address
	 * @throws DuplicateSupplierKeyCoreException
	 * @return nothing.
	 */
  protected void registerSupplier(String key, String name, String address) throws DuplicateSupplierKeyCoreException {
    if(_suppliers.containsKey(key))
      throw new DuplicateSupplierKeyCoreException(key);

    Supplier supplier = new Supplier(key, name, address);
    _suppliers.put(key, supplier);
  }

  protected boolean toggleTransactions(String key) throws UnknownSupplierKeyCoreException {
    if(!_suppliers.containsKey(key))
      throw new UnknownSupplierKeyCoreException(key);

    return _suppliers.get(key).toggleTransactions();
  }

  protected String getSupplierTransactions(String key) throws UnknownSupplierKeyCoreException {
    if(!_suppliers.containsKey(key))
      throw new UnknownSupplierKeyCoreException(key);

    String str = "";  

    for(Map.Entry<Integer, Order> order: _orders.entrySet()) {
      if(order.getValue().getSupplier().getKey().equals(key)) 
        str += order.getValue().toString() + order.getValue().getProducts(); 
    } 
    return str;
  }

  /*-------------------------------------------------TRANSACTIONS------------------------------------------------*/

  /**
  * @return next transaction's key.
  */
  public int getNextTransactionKey() {
    return _nextTransactionKey ++;
  }

  public String getTransaction(int key) throws UnknownTransactionKeyCoreException { 
    if(_sales.containsKey(key) && _sales.get(key).isPayed())
      return _sales.get(key).toString();  

    else if(_sales.containsKey(key) && !_sales.get(key).isPayed())
      return _sales.get(key).toString(this.getDate());

    else if(_orders.containsKey(key))
      return _orders.get(key).toString() + _orders.get(key).getProducts();  

    else 
      throw new UnknownTransactionKeyCoreException(key);
  }

  protected void registerSale(String clientKey, int paymentDeadline, String productKey, int amount) throws UnknownClientKeyCoreException, UnknownProductKeyCoreException, UnavailableProductCoreException {
    if(!_clients.containsKey(clientKey))
      throw new UnknownClientKeyCoreException(clientKey);

    if(!_products.containsKey(productKey))
      throw new UnknownProductKeyCoreException(productKey);

    if(_products.get(productKey).getStock() < amount)
      throw new UnavailableProductCoreException(productKey, amount, _products.get(productKey).getStock());

    int key = getNextTransactionKey();
    Sale sale = new Sale(key, _clients.get(clientKey), paymentDeadline, _products.get(productKey), amount, this.getDate());
    _sales.put(key, sale);

    _products.get(productKey).updateStock(-amount);
  }

  protected void registerOrder(String supplierKey, List<String> productKeys, List<Integer> amounts) throws UnknownSupplierKeyCoreException, UnknownProductKeyCoreException, UnauthorizedSupplierCoreException, WrongSupplierCoreException {
    if(!_suppliers.containsKey(supplierKey))
      throw new UnknownSupplierKeyCoreException(supplierKey);

    if(!_suppliers.get(supplierKey).isActive())
      throw new UnauthorizedSupplierCoreException(supplierKey);

    int key = getNextTransactionKey();
    Order order = new Order(key, _suppliers.get(supplierKey), this.getDate());  

    for(int i = 0; i < productKeys.size(); i++) {  
      if(!_products.containsKey(productKeys.get(i)))
        throw new UnknownProductKeyCoreException(productKeys.get(i));

      if(!_products.get(productKeys.get(i)).getSupplier().getKey().equals(supplierKey))  
        throw new WrongSupplierCoreException(supplierKey, productKeys.get(i));

      order.addProduct(_products.get(productKeys.get(i)), amounts.get(i)); 

      if(_products.get(productKeys.get(i)).getStock() == 0) 
        _products.get(productKeys.get(i)).notify("NEW");
      
      _products.get(productKeys.get(i)).updateStock(amounts.get(i));
    }
    _orders.put(key, order);

    this.updateAvailableBalance(-_orders.get(key).getPrice());
    this.updateAccountingBalance(-_orders.get(key).getPrice());
  }
  
  protected void paySale(int key) throws UnknownTransactionKeyCoreException { 
    if(_orders.containsKey(key))
      return;

    if(!_sales.containsKey(key))
      throw new UnknownTransactionKeyCoreException(key);
    
    if(!_sales.get(key).isPayed()) {
      _sales.get(key).pay(this.getDate());

      this.updateAvailableBalance(_sales.get(key).getPayedPrice());
      this.updateAccountingBalance(_sales.get(key).getPayedPrice());
    }
  }

   /*-------------------------------------------------LOOKUPS------------------------------------------------*/

   public String getProductsUnderPriceLimit(int limit) {
    String str = "";

    for(Map.Entry<String, Product> product: _products.entrySet())
      if(product.getValue().getPrice() < limit)
        str += product.getValue().toString();

    return str;
  }

  public String getPaymentsByClient(String key) throws UnknownClientKeyCoreException {
    if(!_clients.containsKey(key))
      throw new UnknownClientKeyCoreException(key);

    String str = "";

    for(Map.Entry<Integer, Sale> sale: _sales.entrySet())
      if(sale.getValue().getClient().getKey().equals(key) && sale.getValue().isPayed())
        str += sale.getValue().toString();

    return str;
  }
}

