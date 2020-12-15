package woo.app.products;

import pt.tecnico.po.ui.Command;                   
import pt.tecnico.po.ui.DialogException;         
import pt.tecnico.po.ui.Input;
import woo.Storefront;     
import woo.app.exceptions.DuplicateProductKeyException;
import woo.app.exceptions.UnknownSupplierKeyException;
import woo.exceptions.DuplicateProductKeyCoreException;                                                                              
import woo.exceptions.UnknownSupplierKeyCoreException;
/**
 * Register book.
 */
public class DoRegisterProductBook extends Command<Storefront> {

  private Input<String> _key, _supplierKey, _title, _author, _isbn;
  private Input<Integer> _price, _criticalLevel;

  public DoRegisterProductBook(Storefront receiver) {
    super(Label.REGISTER_BOOK, receiver);
    _key = _form.addStringInput(Message.requestProductKey());
    _title = _form.addStringInput(Message.requestBookTitle());
    _author = _form.addStringInput(Message.requestBookAuthor());
    _isbn = _form.addStringInput(Message.requestISBN());
    _price = _form.addIntegerInput(Message.requestPrice());
    _criticalLevel = _form.addIntegerInput(Message.requestStockCriticalValue());
    _supplierKey = _form.addStringInput(Message.requestSupplierKey());
  }

  @Override
  public final void execute() throws DialogException {
    int stock = 0;

    try {
      
      _form.parse();
      _receiver.registerBook(_key.value(), _title.value(), _author.value(), _isbn.value(), _supplierKey.value(), _price.value(), _criticalLevel.value(), stock);   
  
    } catch(DuplicateProductKeyCoreException e){
      throw new DuplicateProductKeyException(_key.value());
    } catch(UnknownSupplierKeyCoreException e){
      throw new UnknownSupplierKeyException(_supplierKey.value());
    }
  }
}

