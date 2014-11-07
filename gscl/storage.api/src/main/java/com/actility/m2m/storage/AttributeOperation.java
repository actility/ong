package com.actility.m2m.storage;

/**
 * Represents an atomic operation on a single attribute.
 */
public interface AttributeOperation {
    /**
     * Operation that sets an attribute value
     */
    public static final int TYPE_SET = 0;
    /**
     * Operation that unsets an attribute value
     */
    public static final int TYPE_UNSET = 1;

    /**
     * Gets the operation type (either {@link #TYPE_SET} or {@link #TYPE_UNSET})
     *
     * @return The operation type
     */
    public int getType();

    /**
     * Gets the attribute on which the operation will be performed.
     *
     * @return The attribute name
     */
    public String getName();

    /**
     * Gets the value associated with the operation (could be <code>null</code>)
     *
     * @return The value associated with the operation
     */
    public Object getValue();

}
