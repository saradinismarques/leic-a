package woo;

import java.io.Serializable;

public class SelectionState extends ClientState implements Serializable {

	private static final long serialVersionUID = 202012061217L;

    public SelectionState(Client client) {
		super(client);
    }

    @Override
    public double getPriceP1(double currentPrice, int daysPassed) {
        return 0.9 * currentPrice;
    }

    @Override
	public double getPriceP2(double currentPrice, int daysPassed) {
        if(-daysPassed >= 2)
            return 0.95 * currentPrice;
        else
            return currentPrice;
    }

    @Override
	public double getPriceP3(double currentPrice, int daysPassed) {
        if(daysPassed > 1)
            return (0.02 * currentPrice) * daysPassed + currentPrice;
        else
            return currentPrice;
    }

    @Override
	public double getPriceP4(double currentPrice, int daysPassed) {
        return (0.05 * currentPrice) * daysPassed + currentPrice;
    }

    @Override
    public void updatePoints(double payedPrice, int daysPassed, double points) {
        if(daysPassed <= 0) 
            _client.setPoints(_client.getPoints() + 10 * payedPrice);
        else if(daysPassed > 2)
            _client.setPoints(_client.getPoints() * 0.10);
    }
    
    @Override
    public void updateState(int daysPassed, double points) {
        if(daysPassed > 2)
            _client.setState(new NormalState(_client));
    
        else if(points > 25000)
            _client.setState(new EliteState(_client));
    }

    @Override
    public String printState() {
        return "SELECTION";
    }
}

