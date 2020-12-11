package woo.app.transactions;

import pt.tecnico.po.ui.Command;       
import pt.tecnico.po.ui.DialogException;               
import pt.tecnico.po.ui.Input;                      
import woo.Storefront;  
import woo.app.exceptions.UnknownClientKeyException;
import woo.app.exceptions.UnknownProductKeyException;
import woo.app.exceptions.UnavailableProductException;
import woo.exceptions.UnknownClientKeyCoreException;
import woo.exceptions.UnknownProductKeyCoreException;
import woo.exceptions.UnavailableProductCoreException;
/*
 * Register sale.
 */
public class DoRegisterSaleTransaction extends Command<Storefront> {

  private Input<String> _clientKey, _productKey;
  private Input<Integer> _paymentDeadline, _amount;

  public DoRegisterSaleTransaction(Storefront receiver) {
    super(Label.REGISTER_SALE_TRANSACTION, receiver);
    _clientKey = _form.addStringInput(Message.requestClientKey());
    _paymentDeadline = _form.addIntegerInput(Message.requestPaymentDeadline());
    _productKey = _form.addStringInput(Message.requestProductKey());
    _amount = _form.addIntegerInput(Message.requestAmount());
  }

  @Override
  public final void execute() throws DialogException {
    try {
      
      _form.parse();
      _receiver.registerSale(_clientKey.value(), _paymentDeadline.value(), _productKey.value(), _amount.value());   
  
    } catch(UnknownClientKeyCoreException e){
      throw new UnknownClientKeyException(_clientKey.value());
    } catch(UnknownProductKeyCoreException e){
      throw new UnknownProductKeyException(_productKey.value());
    } catch(UnavailableProductCoreException e){
      throw new UnavailableProductException(_productKey.value(), _amount.value(), e.getAvailable());
    } 
   
  }

}
