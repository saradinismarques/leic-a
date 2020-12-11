package woo;

import java.io.Serializable;

public abstract class ClientState implements Serializable {

	private static final long serialVersionUID = 202012061210L;

	protected Client _client;
	
	public ClientState(Client client) {
		_client = client;
	}
	
	public abstract double getPriceP1(double currentPrice, int daysPassed);

	public abstract double getPriceP2(double currentPrice, int daysPassed);

	public abstract double getPriceP3(double currentPrice, int daysPassed);

	public abstract double getPriceP4(double currentPrice, int daysPassed);

	public abstract void updatePoints(double payedPrice, int daysPassed, double points);
	
	public abstract void updateState(int daysPassed, double points);

	public abstract String printState();
}
