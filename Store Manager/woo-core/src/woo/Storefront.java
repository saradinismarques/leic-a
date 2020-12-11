package woo;

import java.io.*;
import woo.exceptions.*;
import java.util.List;   
import java.util.ArrayList;
/**
 * Storefront: façade for the core classes.
 */
public class Storefront {

  /** Current filename. */
  private String _filename = "";

  /** The actual store. */
  private Store _store = new Store();


  /*-------------------------------------------------MAIN-------------------------------------------------*/

  public String getFileName(){
    return _filename;
  }

  /**
   * @throws IOException
   * @throws FileNotFoundException
   * @throws MissingFileAssociationException
   */
  public void save() throws IOException, FileNotFoundException, MissingFileAssociationException {
    if(_filename.isEmpty()){
			throw new MissingFileAssociationException();
    }
    ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(_filename));

      oos.writeObject(_store);  
      oos.close();  
  }

  /**
   * @param filename
   * @throws MissingFileAssociationException
   * @throws IOException
   * @throws FileNotFoundException
   */
  public void saveAs(String filename) throws MissingFileAssociationException, FileNotFoundException, IOException {
    _filename = filename;
    save();
  }

  /**
   * @param filename
   * @throws UnavailableFileException
   */
  public void load(String filename) throws UnavailableFileException {
    _filename = filename;
		try{
    ObjectInputStream ois = new ObjectInputStream(new BufferedInputStream(new FileInputStream(filename)));
    _store = (Store) ois.readObject();
    ois.close();
    }catch(IOException | ClassNotFoundException e){
      throw new UnavailableFileException(filename);
    }
  }

  /**
   * @param textfile
   * @throws ImportFileException
   */
  public void importFile(String textfile) throws ImportFileException {
    try {
      _store.importFile(textfile);
    } catch (IOException | BadEntryException | DuplicateProductKeyCoreException | UnknownSupplierKeyCoreException |  UnknownServiceTypeCoreException | UnknownServiceLevelCoreException | DuplicateClientKeyCoreException | DuplicateSupplierKeyCoreException | UnknownClientKeyCoreException e) {
      throw new ImportFileException(textfile);
    }
  }

  public int getDate(){
		return _store.getDate();
	}

  public void advanceDate(int daysToAdvance) throws InvalidDateCoreException{
		_store.advanceDate(daysToAdvance);
  }

  public int getAvailableBalance() {
    return _store.getAvailableBalance();
  }
  
  public int getAccountingBalance() {
    return _store.getAccountingBalance();
  }

  /*-------------------------------------------------PRODUCTS------------------------------------------------*/

  public String getAllProducts(){
    return _store.getAllProducts();
  }

  public void registerBox(String key, String serviceType, String supplierKey, int price, int criticalLevel, int stock) throws DuplicateProductKeyCoreException, UnknownSupplierKeyCoreException, UnknownServiceTypeCoreException {
    _store.registerBox(key, serviceType, supplierKey, price, criticalLevel, stock);
  }

  public void registerContainer(String key, String serviceType, String serviceLevel, String supplierKey, int price, int criticalLevel, int stock) throws DuplicateProductKeyCoreException, UnknownSupplierKeyCoreException, UnknownServiceTypeCoreException, UnknownServiceLevelCoreException {
    _store.registerContainer(key, serviceType, serviceLevel, supplierKey, price, criticalLevel, stock);
  }

  public void registerBook(String key, String title, String author, String isbn, String supplierKey, int price, int criticalLevel, int stock) throws DuplicateProductKeyCoreException, UnknownSupplierKeyCoreException {
    _store.registerBook(key, title, author, isbn, supplierKey, price, criticalLevel, stock);
  }

  public void changePrice(String key, int newPrice) throws UnknownProductKeyCoreException {
    _store.changePrice(key, newPrice);
  }

  /*-------------------------------------------------CLIENTS------------------------------------------------*/

  public String getClient(String key) throws UnknownClientKeyCoreException {
    return _store.getClient(key);
  }

  public String getAllClients(){
    return _store.getAllClients();
  }

  public void registerClient(String key, String name, String address) throws DuplicateClientKeyCoreException{
    _store.registerClient(key, name, address);
  }

  public boolean toggleNotifications(String clientKey, String productKey) throws UnknownClientKeyCoreException, UnknownProductKeyCoreException {
    return _store.toggleNotifications(clientKey, productKey);
  }

  public String getClientTransactions(String key) throws UnknownClientKeyCoreException {
    return _store.getClientTransactions(key);
  }
  
  /*-------------------------------------------------SUPPLIERS------------------------------------------------*/

  public String getAllSuppliers() {
    return _store.getAllSuppliers();
  }  

  public void registerSupplier(String key, String name, String address) throws DuplicateSupplierKeyCoreException {
    _store.registerSupplier(key, name, address);
  }

  public boolean toggleTransactions(String key) throws UnknownSupplierKeyCoreException {
    return _store.toggleTransactions(key);
  }

  public String getSupplierTransactions(String key) throws UnknownSupplierKeyCoreException {
    return _store.getSupplierTransactions(key);
  }

  /*-------------------------------------------------TRANSACTIONS------------------------------------------------*/

  public String getTransaction(int key) throws UnknownTransactionKeyCoreException { 
    return _store.getTransaction(key);
  }

  public void registerSale(String clientKey, int paymentDeadline, String productKey, int amount) throws UnknownClientKeyCoreException, UnknownProductKeyCoreException, UnavailableProductCoreException {
    _store.registerSale(clientKey, paymentDeadline, productKey, amount);
  }

  public void registerOrder(String supplierKey, List<String> productKeys, List<Integer> amounts) throws UnknownSupplierKeyCoreException, UnknownProductKeyCoreException, UnauthorizedSupplierCoreException, WrongSupplierCoreException {
    _store.registerOrder(supplierKey, productKeys, amounts);
  }

  public void paySale(int key) throws UnknownTransactionKeyCoreException {
    _store.paySale(key);
  }
  
  /*-------------------------------------------------LOOKUPS------------------------------------------------*/

  public String getProductsUnderPriceLimit(int limit) {
    return _store.getProductsUnderPriceLimit(limit);
  }

  public String getPaymentsByClient(String key) throws UnknownClientKeyCoreException {
    return _store.getPaymentsByClient(key);
  }
}

