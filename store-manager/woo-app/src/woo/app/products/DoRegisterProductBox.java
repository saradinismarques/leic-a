package woo.app.products;

import pt.tecnico.po.ui.Command;                                                   
import pt.tecnico.po.ui.DialogException;                                         
import pt.tecnico.po.ui.Input;                                                   
import woo.Storefront;
import woo.app.exceptions.DuplicateProductKeyException;
import woo.app.exceptions.UnknownSupplierKeyException;
import woo.exceptions.DuplicateProductKeyCoreException;                                                                              
import woo.exceptions.UnknownSupplierKeyCoreException;      
import woo.app.exceptions.UnknownServiceTypeException;
import woo.exceptions.UnknownServiceTypeCoreException;                                                                             
/**
 * Register box.
 */
public class DoRegisterProductBox extends Command<Storefront> {

  private Input<String> _key, _supplierKey, _serviceType;
  private Input<Integer> _price, _criticalLevel;

  public DoRegisterProductBox(Storefront receiver) {
    super(Label.REGISTER_BOX, receiver);
    _key = _form.addStringInput(Message.requestProductKey());
    _price = _form.addIntegerInput(Message.requestPrice());
    _criticalLevel = _form.addIntegerInput(Message.requestStockCriticalValue());
    _supplierKey = _form.addStringInput(Message.requestSupplierKey());
    _serviceType = _form.addStringInput(Message.requestServiceType());

  }

  @Override
  public final void execute() throws DialogException {
    int stock = 0;

    try{

      _form.parse();
      _receiver.registerBox(_key.value(), _serviceType.value(), _supplierKey.value(), _price.value(), _criticalLevel.value(), stock);
         
    } catch(DuplicateProductKeyCoreException e){
      throw new DuplicateProductKeyException(_key.value());
    } catch(UnknownSupplierKeyCoreException e){
      throw new UnknownSupplierKeyException(_supplierKey.value());
    } catch(UnknownServiceTypeCoreException e){
      throw new UnknownServiceTypeException(_serviceType.value());
    }
  }
}
