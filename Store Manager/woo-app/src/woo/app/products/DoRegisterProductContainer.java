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
import woo.app.exceptions.UnknownServiceLevelException;
import woo.exceptions.UnknownServiceTypeCoreException;                                                                             
import woo.exceptions.UnknownServiceLevelCoreException;                                                                       
/**
 * Register container.
 */
public class DoRegisterProductContainer extends Command<Storefront> {

  private Input<String> _key, _supplierKey, _serviceType, _serviceLevel;
  private Input<Integer> _price, _criticalLevel;

  public DoRegisterProductContainer(Storefront receiver) {
    super(Label.REGISTER_CONTAINER, receiver);
    _key = _form.addStringInput(Message.requestProductKey());
    _price = _form.addIntegerInput(Message.requestPrice());
    _criticalLevel = _form.addIntegerInput(Message.requestStockCriticalValue());
    _supplierKey = _form.addStringInput(Message.requestSupplierKey());
    _serviceType = _form.addStringInput(Message.requestServiceType());
    _serviceLevel = _form.addStringInput(Message.requestServiceLevel());
  }

  @Override
  public final void execute() throws DialogException {
    int stock = 0;

    try{

      _form.parse();
      _receiver.registerContainer(_key.value(), _serviceType.value(), _serviceLevel.value(), _supplierKey.value(), _price.value(), _criticalLevel.value(), stock); 

    } catch(DuplicateProductKeyCoreException e){
      throw new DuplicateProductKeyException(_key.value());
    } catch(UnknownSupplierKeyCoreException e){
      throw new UnknownSupplierKeyException(_supplierKey.value());
    } catch(UnknownServiceTypeCoreException e){
      throw new UnknownServiceTypeException(_serviceType.value());
    } catch(UnknownServiceLevelCoreException e){
      throw new UnknownServiceLevelException(_serviceLevel.value());
    }
  
  }
}
