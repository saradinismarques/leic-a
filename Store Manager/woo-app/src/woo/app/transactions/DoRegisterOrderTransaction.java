package woo.app.transactions;

import pt.tecnico.po.ui.Command;   
import pt.tecnico.po.ui.DialogException;   
import pt.tecnico.po.ui.Input;   
import pt.tecnico.po.ui.Form;  
import java.util.List;   
import java.util.ArrayList;
import woo.Storefront;     
import woo.app.exceptions.UnknownSupplierKeyException;
import woo.app.exceptions.UnknownProductKeyException;
import woo.exceptions.UnknownSupplierKeyCoreException;
import woo.exceptions.UnknownProductKeyCoreException;
import woo.app.exceptions.UnauthorizedSupplierException;
import woo.app.exceptions.WrongSupplierException;
import woo.exceptions.UnauthorizedSupplierCoreException;
import woo.exceptions.WrongSupplierCoreException ;
/**
 * Register order.
 */
public class DoRegisterOrderTransaction extends Command<Storefront> {

  private Input<String> _supplierKey, _productKey;
  private Input<Integer> _amount;


  public DoRegisterOrderTransaction(Storefront receiver) {
    super(Label.REGISTER_ORDER_TRANSACTION, receiver);
    _supplierKey = _form.addStringInput(Message.requestSupplierKey());
    _productKey = _form.addStringInput(Message.requestProductKey());
    _amount = _form.addIntegerInput(Message.requestAmount());
  }

  @Override
  public final void execute() throws DialogException {  
    Input<String>  otherProductKey;
    Input<Integer>  otherAmount; 
    Input<Boolean> moreProducts;
    List<String> productKeys = new ArrayList<String>();
    List<Integer> amounts = new ArrayList<Integer>();
    Form secondForm = new Form();

    try {

      _form.parse();
      productKeys.clear();
      amounts.clear();

      productKeys.add(_productKey.value());
      amounts.add(_amount.value());
      
      moreProducts = secondForm.addBooleanInput(Message.requestMore());
      secondForm.parse();
             
      while(moreProducts.value()) {

        secondForm.clear();
        otherProductKey = secondForm.addStringInput(Message.requestProductKey());
        otherAmount = secondForm.addIntegerInput(Message.requestAmount());
        
        secondForm.parse();
        productKeys.add(otherProductKey.value());
        amounts.add(otherAmount.value());

        secondForm.clear();
        moreProducts = secondForm.addBooleanInput(Message.requestMore());
        secondForm.parse();
      } 
      
      _receiver.registerOrder(_supplierKey.value(), productKeys, amounts);

    } catch(UnknownSupplierKeyCoreException e) {
        throw new UnknownSupplierKeyException(_supplierKey.value());
    } catch(UnknownProductKeyCoreException e) {
        throw new UnknownProductKeyException(e.getProductKey());
    } catch(UnauthorizedSupplierCoreException e) {
        throw new UnauthorizedSupplierException(_supplierKey.value());
    } catch(WrongSupplierCoreException e) {
        throw new WrongSupplierException(_supplierKey.value(), e.getPKey());  
    }  
  }
}